# PKG-CRACKER

**PKG-CRACKER** is a program that exploits brute force attacks on the `.pkg` files of PS4 and PS5 consoles.  
The idea for this program was inspired by the original [Waste-Ur-Time](https://github.com/HoppersPS4/Waste_Ur_Time) project created by [HoppersPS4](https://github.com/HoppersPS4).

## Why PKG-CRACKER?

The original program, although useful, suffers from some limitations that I have tried to solve with this new version. In particular, I have introduced:

- **Support for multithreading:** Makes better use of the CPU to speed up the key verification process.
- **Elimination of unusable keys:** Automatically removes keys that have already been used, improving program efficiency.
- **Improved project structure:** The new version adopts a more modular and organized structure, increasing code maintainability compared to the original version, which was written entirely in a single file.


## Key features.

- **Advanced brute force:** Tests key combinations efficiently.
- **Multithreading:** Accelerates processing through multi-core processing power.
- **Intelligent key management:** Avoid wasting time by eliminating already tested keys.
- **Modular structure:** Facilitates project understanding and expansion.

## Requirements.

To use PKG-CRACKER, make sure you have:

- **Operating system:** Windows 10/11
- **Tools required:**
  - `orbis-pub-cmd.exe` (for PS4)
  - `prospero-pub-cmd.exe` (for PS5)

## Installation.

1. Clone the repository:
   ```bash
   git clone https://github.com/tuo-username/PKG-CRACKER.git
   cd PKG-CRACKER
   ```
2. Configure the supporting executable files (``orbis-pub-cmd.exe`` and ``prospero-pub-cmd.exe``) in the project directory.

## Usage

Compile the program using:
```bash
cmake --build . --config Release
```
run the program using(or simply double-click on the executable): 
```bash
./PKG-CRACKER.exe
```

## Contributions

If you have suggestions or would like to contribute to the project, you are welcome! Create a pull request or open an issue in the repository.

## Credits.

- **Inspiration:** [Waste-Ur-Time](https://github.com/HoppersPS4/Waste_Ur_Time) by [HoppersPS4](https://github.com/HoppersPS4)
- **Developer:** [Seregonwar](https://github.com/SeregonWar)

## License.

This project is distributed under the MIT license. See the `LICENSE` file for more details.
