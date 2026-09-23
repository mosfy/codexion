# codexion
another dumb project from 42....

### 🛠️ Phase 1: Setup & Initialization
- [X] Initialize Git repository.
- [X] Create `Makefile` with rules: `$(NAME)`, `all`, `clean`, `fclean`, `re`.
- [X] Ensure `Makefile` uses flags: `-Wall -Wextra -Werror -pthread` and prevents relinking.
- [X] Create basic `.c` and `.h` files conforming to the 42 Norm.

### 📥 Phase 2: Argument Parsing & Validation
- [X] Check argument count (exactly 8 arguments).
- [X] Implement a safe parser (e.g., custom `ft_atoi`).
- [X] Validate arguments (no negative numbers, integers only).
- [X] Ensure `scheduler` argument is strictly `fifo` or `edf`.

### 🏗️ Phase 3: Data Structures (No Global Variables)
- [X] Create `t_dongle` struct (mutex, condition variable, cooldown timestamp).
- [X] Create `t_coder` struct (ID, pthread_t, pointers to L/R dongles, last compile time, compile count).
- [X] Create `t_simulation` struct (arguments, print mutex, stop flag, arrays pointers).
- [X] Implement a custom Priority Queue (Heap) for FIFO/EDF scheduling.

### ⚙️ Phase 4: Memory Allocation & Thread Primitives Setup
- [X] Allocate memory (`malloc`) for coders and dongles arrays.
- [X] Initialize all `pthread_mutex_t` (dongles, printing, simulation state).
- [X] Initialize all `pthread_cond_t`.
- [X] Map coders to their respective left and right dongles (circular logic).

### 🧠 Phase 5: Resource Arbitration (Scheduling)
- [ ] Implement `fifo` scheduling logic (first come, first served).
- [ ] Implement `edf` scheduling logic (Earliest Deadline First based on burnout time).
- [ ] Handle tie-breakers in EDF for determinism.
- [ ] Implement dongle cooldown logic.

### 🔄 Phase 6: Coder Thread Routine
- [ ] Create threads for coders using `pthread_create`.
- [ ] Implement safe printing function (locks mutex, checks if simulation is running, prints).
- [ ] Implement routine: request dongles -> `compiling` (sleep) -> release dongles.
- [ ] Implement routine continuation: `debugging` (sleep) -> `refactoring` (sleep).

### 👁️ Phase 7: Monitor Thread
- [ ] Create monitor thread (or use main thread) to continuously check coders.
- [ ] Detect burnout: `current_time - last_compile_time > time_to_burnout`.
- [ ] Ensure burnout log is printed within 10ms of actual burnout.
- [ ] Detect simulation end if all coders reach `number_of_compiles_required`.
- [ ] Set simulation stop flag safely to halt coder threads.

### 🧹 Phase 8: Cleanup
- [ ] Wait for all threads to finish using `pthread_join`.
- [ ] Destroy all mutexes (`pthread_mutex_destroy`).
- [ ] Destroy all condition variables (`pthread_cond_destroy`).
- [ ] Free all allocated memory (zero leaks allowed).

### 📝 Phase 9: README.md Requirements
- [ ] Add the mandatory italic first line (`This project has been created as part of the 42 curriculum...`).
- [ ] Write Description and Instructions sections.
- [ ] Write Resources section (mentioning LLNL Pthreads tutorial and AI usage).
- [ ] Write "Blocking cases handled" section.
- [ ] Write "Thread synchronization mechanisms" section.
number_of_coders 
time_to_burnout 
time_to_compile
time_to_debug
time_to_refactor
number_of_compiles_required
dongle_cooldown 
scheduler

valgrind --tool=helgrind

------


fifo = 0
edf = 1

schelduer = 1// Mode EDF

mutex_print evite que deux thread ecrive en meme temps

C'est quoi une data race ?
une data race cest lorsque plusiers thread utilise les meme donner et que cela ne donne pas le resulta voulue, exemple i = 100 thread 1 ajout 20 a la variable i thread 2 ajout 10, a la fin sans aucun protection on aura i = 110 au lieu de 130

Comment tu verifies tes data races ?

- Est-ce que tu dois faire des trucs a la compilation ?
lors de la compilation le -g3* du make file permet de simplifier le debuggage et donne plusieur info notament ou la datarace a lieu

*le -g fonctionne mais le g3 donne plus d'info en generale donc je le prefere

- Est-ce que tu dois faire des trucs a l'execution du programme ?
utilser valgrind --tool=helgrind ./codexion permet de verifier si il y a des data race et avec le -g3 on voit ou elle a lieux

- Quelles limites ?
le valgrinnd ralentit le programe et encore plus avec le --tool=helgrind,
on peut passer de 10 a 50 fois plus lent

- Pourquoi ?
tout simplement car valgrind execute le programe de facon virtuel pour pouvoir avoir acces tres facilment a la memoir contrairment a un proesseur physique ou cela est tres dure. De plus --tool=helgrind verifie que chaque varibale n est pas utilise par un autre thread ce qui est extrement gourmand

Mets les sources qui t'ont aide a comprendre.
https://hpc-tutorials.llnl.gov/posix/#table-of-contents
abosc
ancourti


