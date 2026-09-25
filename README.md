# codexion — Audit vs sujet (v1.5) & plan de patch

> Ce fichier récapitule **tous les écarts** entre le code actuel et le sujet
> `Codexion` (v1.5), classés par priorité, avec pour chacun : fichier/ligne,
> problème, preuve et correctif proposé.
> ⚠️ Le README **final** rendu à l'évaluation doit être en **anglais** et suivre
> le squelette de la section [9](#9-readme-final-obligatoire).
> (Les anciennes notes de ce fichier sont dans l'historique git : `git show 727c0a6:README.md`.)

---

## 0. État actuel en un coup d'œil

| Test | Résultat attendu | Résultat actuel |
|---|---|---|
| `make` sur macOS (clang) | compile | ❌ `coder.c:91: parameter 'now' set but not used` |
| `./codexion 4 410 200 200 200 3 0 fifo` | simulation / fin propre | ❌ **bloqué à vie, 0 ligne affichée** |
| `./codexion 1 800 200 200 200 3 0 edf` | 1 dongle pris puis `800 1 burned out` | ❌ compile en boucle avec un seul dongle |
| `./codexion 2 800 200 200 200 1 -5 fifo` | erreur d'argument | ❌ accepté (argv[7] jamais validé) puis bloqué |
| `./codexion 2 100 200 100 100 5 0 fifo` | `burned out` ≤ 10 ms après 100 ms | ❌ aucun thread moniteur, jamais de burnout |
| Sortie | uniquement les 5 messages du sujet | ❌ `mutex lock` / `mutex unlock` affichés |

---

## 1. 🔴 Bloquants (le programme ne fonctionne pas)

### 1.1 Deadlock global dans `take_dongles` — `coder.c:113-134`
- **Problème** : le codeur verrouille `sim->mutex` puis fait une **attente active** (`while`) *sans jamais relâcher le mutex*. Le codeur qui tient les dongles doit prendre ce même mutex dans `release_dongle` (`coder.c:40`) → il ne peut jamais les rendre → tout le monde est bloqué.
- **Correctif** : remplacer la boucle active par `pthread_cond_wait` (qui relâche le mutex pendant l'attente), ou mieux `pthread_cond_timedwait` pour se réveiller quand le cooldown expire :
  ```c
  pthread_mutex_lock(&sim->mutex);
  /* push dans les deux files */
  while (!is_simulation_stopped(sim) && !can_take(coder, get_time_in_ms()))
      cond_timedwait_ms(&sim->condition_variable, &sim->mutex, 1);
  ```
  (petit helper qui construit un `struct timespec` à `now + 1 ms`.)

### 1.2 Pas de thread moniteur — sujet ch. VI
- **Problème** : `stop_flag` n'est **jamais** mis à 1, aucun message `burned out`, aucune fin quand tout le monde a atteint `number_of_compiles_required`.
- **Correctif** : créer un thread `monitor` dans `main` après les codeurs :
  ```c
  while (1) {
      for each coder:
          lock(mutex_stop); last = c->last_compile_time; unlock;
          if (now - last >= time_to_burnout) {
              lock(mutex_print);
              set_stop(sim);                       // sous mutex_stop
              printf("%lld %d burned out\n", now - start, c->id);
              unlock(mutex_print);
              broadcast(condition_variable);        // réveiller ceux qui attendent
              return NULL;
          }
      if (tous compile_count >= required) { set_stop; broadcast; return NULL; }
      usleep(1000);                                 // précision < 10 ms
  }
  ```
  ⚠️ Le message `burned out` doit s'afficher **même si** `print_status` refuse d'afficher après l'arrêt : l'imprimer directement sous `mutex_print` **avant/en même temps** que la mise à 1 du flag.
  ⚠️ Ordre des verrous : `print_status` prend `mutex_print` → `mutex_stop`. Le moniteur doit respecter le **même ordre** (sinon deadlock).

### 1.3 Le heap n'arbitre rien — `coder.c:91-134`
- **Problème** : on `heap_push` puis on attend seulement `!is_in_use`. Le codeur prioritaire n'est jamais vérifié, et `heap_pop` retire **la racine**, qui n'est pas forcément le codeur courant → files corrompues, FIFO/EDF non respectés (exigence *Fair arbitration is mandatory*).
- **Correctif** : un codeur ne peut prendre que s'il est **en tête des deux files** :
  ```c
  static int can_take(t_coder *c, long long now)
  {
      t_dongle *l = c->l_dongle, *r = c->r_dongle;
      return (!l->is_in_use && !r->is_in_use
          && now >= l->cooldown_timestamp && now >= r->cooldown_timestamp
          && l->queue.tree[0].coder_id == c->id
          && r->queue.tree[0].coder_id == c->id);
  }
  ```
  Et ajouter `heap_remove(t_heap *h, int coder_id)` pour sortir le codeur des files si la simulation s'arrête pendant l'attente (actuellement il y reste).

### 1.4 Cooldown jamais appliqué — `coder.c:91-102`
- `cooldown_timestamp` est écrit dans `release_dongle` mais **jamais lu**. `now++` ne sert à rien (et casse la compilation clang). → intégré dans `can_take` ci-dessus.

### 1.5 Messages `has taken a dongle` absents — `coder.c`
- Le sujet impose **2 lignes** `X has taken a dongle` avant chaque `X is compiling`. → après la prise, faire `print_status(coder, "has taken a dongle")` ×2 (hors `sim->mutex` ou dans, mais toujours via `mutex_print`).

### 1.6 Logs de debug parasites — `coder.c:114` et `coder.c:135`
- `printf("mutex lock\n")` / `printf("mutex unlock\n")` → **à supprimer** (le second est même hors mutex, donc peut s'entrelacer).

### 1.7 Cas 1 codeur — `init1.c:66` + `coder.c`
- `r_dongle = &dongles[(0 + 1) % 1]` = le **même** dongle que `l_dongle` → le codeur compile avec un seul dongle.
- **Attendu** : 1 seul dongle sur la table, le codeur le prend (`0 1 has taken a dongle`), ne peut jamais compiler, et `time_to_burnout 1 burned out`.
- **Correctif** : dans `coder_tread`, si `l_dongle == r_dongle` : afficher la prise, puis `ft_usleep(time_to_burnout, sim)` et sortir (le moniteur affiche le burnout).

### 1.8 Ne compile pas avec `-Werror` (clang) — `coder.c:91,98`
- `now` modifié mais jamais utilisé → erreur. Réglé par 1.3/1.4.

---

## 2. 🟠 Parsing des arguments — `init1.c:15-37`, `src/ft_atoi.c`, `src/ft_isdigit.c`

| # | Problème | Correctif |
|---|---|---|
| 2.1 | Boucle `while (i != 7)` → **`argv[7]` (dongle_cooldown) jamais validé** (`-5`, `abc` acceptés) | boucler `i = 1..7` |
| 2.2 | `ft_atoi(...) <= 0` refuse `dongle_cooldown = 0` (valeur légitime) | autoriser `0` pour le cooldown (et décider/justifier pour `number_of_compiles_required`) |
| 2.3 | Pas de détection de dépassement : `99999999999` passe `ft_isdigit` puis `ft_atoi` déborde (UB, valeur aléatoire) | parser en `long`, refuser si `> INT_MAX` ou longueur > 10 |
| 2.4 | `argv[8] = "0"` / `"1"` puis `ft_atoi(argv[8])` : bricolage | stocker directement `scheduler_type = (strcmp(argv[8], "edf") == 0)` dans `simulation_init` |
| 2.5 | Aucun message d'erreur, codes retour incohérents (-1, -4, -2, 1) | afficher `Error: invalid arguments` + usage sur `stderr` (`fprintf` autorisé) |
| 2.6 | `ft_isdigit` refuse `+42` (ok) mais le nom est trompeur | renommer `is_number` (lisibilité en défense) |

---

## 3. 🟠 Temps — `init2.c:15-21`

- `int get_time_in_ms(void)` : `tv_sec * 1000` ≈ 1,7·10¹² **ne tient pas dans un `int`** → tronqué. Les soustractions marchent « par chance » jusqu'au prochain wrap.
  → **retourner `long long`** (et mettre à jour le prototype `header.h:93`).
- `ft_usleep` : OK (pas de 500 µs, sort si stop). Garder.

---

## 4. 🟠 Data races (à vérifier avec `valgrind --tool=helgrind` / `-fsanitize=thread`)

| Donnée | Écrite par | Lue par | Protection actuelle | À faire |
|---|---|---|---|---|
| `coder->compile_count` | codeur (`coder.c:67`) | moniteur (à venir) | ❌ aucune | incrémenter sous `mutex_stop` (ou un `mutex_meal` par codeur) |
| `coder->last_compile_time` | codeur (`coder.c:62`, sous `mutex_stop`) | codeur (`coder.c:111`, sans lock) + moniteur | ⚠️ partielle | lire aussi sous le même mutex |
| `is_in_use`, `cooldown_timestamp`, files | codeurs | codeurs | `sim->mutex` global | OK mais voir 5.1 |
| `stop_flag` | moniteur | tous | `mutex_stop` | OK |

- `last_compile_time` doit être mis à jour **au début** de la compilation (c'est le cas) — garder.

---

## 5. 🟡 Conformité au sujet (moins visible mais évaluable)

### 5.1 Un mutex **par dongle** — sujet : *« protect each dongle's state with a mutex »*
- Actuellement : un seul `sim->mutex` pour tous les dongles. Fonctionne mais **ne correspond pas à la lettre** du sujet.
- Option A (conforme) : `pthread_mutex_t mutex;` dans `t_dongle` (+ éventuellement un `pthread_cond_t`), prise ordonnée (dongle d'indice le plus petit d'abord) pour éviter le deadlock.
- Option B (plus simple) : garder le mutex global **et le justifier** dans le README (section *Thread synchronization mechanisms*). Risqué en défense.

### 5.2 Fin sur `number_of_compiles_required` — `coder.c:83-85`
- Chaque codeur s'arrête dès **son** quota, alors que le sujet dit : la simulation s'arrête quand **tous** ont compilé ≥ N fois. → laisser le moniteur décider de l'arrêt global ; les codeurs bouclent jusqu'à `stop_flag`.

### 5.3 FIFO en millisecondes — `coder.c:110`
- `arrival_time` en ms → nombreuses égalités, départagées par l'id (pas vraiment « ordre d'arrivée »). → utiliser un **compteur de tickets** (`sim->ticket++` sous `sim->mutex`) comme `arrival_time`.

### 5.4 Fonction interdite `exit()` — `main.c:117`
- `exit` n'est **pas** dans la liste des fonctions autorisées. → nettoyer, joindre les threads déjà créés et `return (1)`.

### 5.5 Allocation non vérifiée — `simulation.c:54-57`, `main.c:108`, `main.c:37`
- `simulation_init` retourne `void` : si `ft_calloc` échoue, `main` continue et déréférence `NULL` → segfault (= 0 à l'éval).
- `heap_init` ne vérifie pas `ft_calloc`.
- → faire retourner `int` à `simulation_init`/`heap_init`, libérer ce qui a été alloué et quitter proprement.

---

## 6. 🔴 Fuites mémoire & destruction — `main.c:127`, `simulation.c:15-36`

- `main` fait seulement `free(simulation)` → **fuite** de `coders`, `dongles`, et des `tree` de chaque heap.
- `clean_simulation` existe mais **n'est jamais appelée**, et ne détruit ni `sim->mutex` ni `sim->condition_variable`.
- → à la fin de `main` : join codeurs + moniteur, puis `clean_simulation(simulation)` (qui fait déjà le `free(sim)`) complétée par :
  ```c
  pthread_mutex_destroy(&sim->mutex);
  pthread_cond_destroy(&sim->condition_variable);
  /* + mutex par dongle si 5.1 option A */
  ```
- Vérifier : `valgrind --leak-check=full --show-leak-kinds=all ./codexion 4 800 200 200 200 3 10 edf`

---

## 7. 🟡 Norme 42 (0 direct si erreur)

`norminette` n'est pas installé sur cette machine → à lancer sur un poste 42. Erreurs probables :

| Fichier | Problème |
|---|---|
| `coder.c` | **8 fonctions** (max 5) → séparer : `coder.c` (routine), `dongle.c` (take/release/can_take), `print.c`/`monitor.c` |
| `header.h` | pas de **include guard** (`#ifndef HEADER_H`) ; prototype `heap_destroy` **en double** (l. 82 et 96) ; `main` n'a pas besoin de prototype |
| `src/ft_bzero.c`, `src/ft_calloc.c` | **ligne vide en double** après le header 42 ; pas de `\n` final |
| `Makefile` | `.PHONY` sans `\n` final (pas norme mais propre) |
| `main.c` | contient le code du heap (`heap_push`, `heap_sift_down`, `is_higher_priority`) → le déplacer dans `heap.c` / `heap_utils.c` pour la lisibilité |
| divers | faute de frappe `coder_tread` → `coder_thread` |

---

## 8. 🟡 Dépôt & Makefile

- **Binaire `codexion` commité** (ELF Linux) → `git rm --cached codexion` et ajouter `codexion` + `obj/` au `.gitignore`.
- `.gitignore` ignore `demo/` mais le dossier s'appelle **`.demo/`** → il est commité (fichiers de test hors sujet, peuvent déclencher la norme). Le retirer ou l'ignorer. Idem `.vscode/`.
- `valgrindrc` : pas utile au rendu → retirer ou ignorer.
- `Makefile:22` : `$(LIBFT)` n'est défini nulle part (libft **non autorisée**) → supprimer.
- `Makefile:5` : `-g3` → ok pour debug, mais retirer avant rendu (le sujet demande `-Wall -Wextra -Werror -pthread`).
- Pas de dépendance sur `header.h` : modifier le header ne recompile rien → ajouter `$(OBJDIR)/%.o: %.c header.h`.
- Relink : OK.

---

## 9. README final (obligatoire)

À remplacer ce fichier une fois les patchs faits. **En anglais**, avec au minimum :

```markdown
*This project has been created as part of the 42 curriculum by tfrances, abosc.*

## Description
## Instructions
## Resources
   - LLNL POSIX Threads tutorial: https://hpc-tutorials.llnl.gov/posix/
   - man pthread_mutex_lock / pthread_cond_timedwait / gettimeofday
   - How AI was used (which tasks, which parts)
## Blocking cases handled
   - deadlock prevention & Coffman's conditions (which one is broken, and how)
   - starvation prevention (EDF + heap)
   - cooldown handling
   - precise burnout detection (monitor, 1 ms polling, < 10 ms)
   - log serialization (mutex_print)
## Thread synchronization mechanisms
   - pthread_mutex_t: which mutex protects what (dongles, print, stop/monitor state)
   - pthread_cond_t: how coders wait / are woken up
   - examples of prevented race conditions, coder ↔ monitor communication
```

Les notes existantes sur helgrind / data race (anciennes lignes 98-117) sont réutilisables pour la section *Resources* ou la défense.

---

## 10. Ordre de patch conseillé

1. `get_time_in_ms` → `long long` (3) · supprimer les `printf` debug (1.6)
2. Parsing (2.1 → 2.5) · retirer `exit` (5.4) · vérifier les allocs (5.5)
3. `take_dongles` : `cond_timedwait` + `can_take` (tête de file + cooldown) + `heap_remove` (1.1, 1.3, 1.4, 5.3)
4. Messages `has taken a dongle` (1.5) · cas 1 codeur (1.7)
5. Thread moniteur + arrêt global (1.2, 5.2) · protections `compile_count` / `last_compile_time` (4)
6. Cleanup complet + valgrind / helgrind (6)
7. Découpage des fichiers + norminette (7) · ménage du dépôt (8)
8. README final en anglais (9)

### Tests à repasser après patch
```bash
./codexion 1 800 200 200 200 3 0 edf        # 0 has taken a dongle / 800 1 burned out
./codexion 4 410 200 200 200 3 0 fifo       # doit terminer (tous à 3 compiles) ou burnout précis
./codexion 5 800 200 200 200 7 0 edf        # ne doit jamais burn out
./codexion 4 310 200 100 100 5 0 fifo       # burnout attendu vers 310 ms (± 10)
./codexion 2 800 200 200 200 1 -5 fifo      # erreur
./codexion 2 800 200 200 200 1 5 rr         # erreur
valgrind --tool=helgrind ./codexion 5 3000 200 1 200 4 800 edf
```
