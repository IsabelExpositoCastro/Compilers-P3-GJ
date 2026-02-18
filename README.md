
# **P1 – PreSubmission GJ**

## 👥 **Group Members**
- **Alex Martínez**     — Team Leader  (TM1)
- **Adrià Coll**        — Team Leader  (TM2)
- **Isabel Expósito**   — Team Leader  (TM3)
- **Didac Raya**        — Team Leader  (TM4)
- **Margarita Crespo**  — Team Leader  (TM5)
- **Valentyn Korol**    — Team Leader  (TM6)

---

GitHub-Repo: [Compilers_P2_GJ](https://github.com/IsabelExpositoCastro/Compilers-P2_GJ.git)

## 📄 **Overview**
This document contains all the needed information to understand **P2 Final Submission** of the project.

The code done by all of us, is inside **`/src`** folder, organized in different modules to facilitate the team-work and to improve clean code along the project.
Also, we will provide the test_file.txt used to provide the slides results.

---

## **Usage Instructions**

- In order to build the program, just need to do the Ctl+Shift+P and build it.
- The executable will be done on /build folder
- Inside the /build folder, it exists Automatas.txt file, that contains all the automatas information to use.
- To run the executable, do the following command: ./lexer.exe <test_input.txt>
- Finally, in order to change the RELEASE/DEBUG mode, go to preprocesor_variables.h, and modify OUTFORMAT definition


## 📁 **Project Structure**

The project skeleton with all folders and files is the following one:
```
/src
.
│   CMakeLists.txt
│   main.c
│   main.h
│   preprocesor_variables.h
│   utils_files.c
│   utils_files.h
│
├───automatas_module
│       automatas.txt
│       automatonDefinition.c
│       automatonDefinition.h
│       CMakeLists.txt
│
├───counter_module
│       CMakeLists.txt
│       counter.c
│       counter.h
│
├───error_handler_module
│       CMakeLists.txt
│       error_handler.c
│       error_handler.h
│
├───input_handler_module
│       CMakeLists.txt
│       input_handler.c
│       input_handler.h
│       module_args.c
│       module_args.h
│
└───scanner_module
        CMakeLists.txt
        scanner.c
        scanner.h

```
