# Contributing to AiNupiz

First off, thank you for considering contributing to AiNupiz! It's people like you who make AiNupiz such a great tool.

## What is AiNupiz?

**AiNupiz** is a fork of the original [NupizLang programming language](https://github.com/Lemon-chad/NupizLang), enhanced with AI-assisted development and additional modern features including a full HTTP library for web API support.

## How Can I Contribute?

### Reporting Bugs

- Use the [bug report template](.github/ISSUE_TEMPLATE/bug_report.md)
- Include detailed steps to reproduce the issue
- Mention your operating system and compiler version
- If the bug exists in the original NupizLang, please report it there too

### Suggesting Enhancements

- Use the [feature request template](.github/ISSUE_TEMPLATE/feature_request.md)
- Explain why this enhancement would be useful
- Consider if it would benefit the original NupizLang project

### Submitting Code Changes

1. **Fork the repository** on GitHub
2. **Clone your fork** locally
3. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
4. **Make your changes** following the coding standards
5. **Test your changes** thoroughly
6. **Commit your changes** with clear commit messages
7. **Push to your fork** (`git push origin feature/amazing-feature`)
8. **Create a Pull Request** using our template

## Development Setup

### Prerequisites

- C/C++ compiler (GCC 8.1+ or Clang 12.0+)
- Make utility
- Git

### Building

```bash
cd vm
make all
```

### Testing

```bash
# Compile a test file
../build/npz -c test_file.npz -o test_file.bin

# Run the compiled file
../build/npz -r test_file.bin
```

## Coding Standards

### C/C++ Code

- Use consistent indentation (spaces, not tabs)
- Follow the existing naming conventions
- Add comments for complex logic
- Handle memory management carefully
- Include proper error handling

### NupizLang Code

- Use descriptive variable names
- Follow the existing syntax patterns
- Include examples in documentation
- Test your code thoroughly

## Project Structure

```
AiNupiz/
├── compiler/          # NupizLang compiler
├── vm/               # Virtual machine implementation
│   ├── libraries/    # Native libraries (HTTP, std, etc.)
│   ├── compiler/     # VM compiler components
│   └── vm/          # Core VM implementation
├── npzdemos/         # Example programs
└── docs/            # Documentation
```

## Key Areas for Contribution

### 🚀 HTTP Library
- HTTPS support
- JSON parsing
- WebSocket support
- Better error handling

### 🔧 Core Language Features
- Additional standard library functions
- Performance improvements
- Better error messages
- Enhanced debugging tools

### 📚 Documentation
- Code examples
- Tutorials
- API documentation
- Best practices guide

### 🧪 Testing
- Unit tests
- Integration tests
- Performance benchmarks
- Cross-platform testing

## Communication

- **Issues**: Use GitHub Issues for bugs and feature requests
- **Discussions**: Use GitHub Discussions for questions and ideas
- **Pull Requests**: Submit code changes via PRs

## Code of Conduct

- Be respectful and inclusive
- Focus on the code and technical merits
- Help others learn and grow
- Give credit where credit is due

## License

By contributing to AiNupiz, you agree that your contributions will be licensed under the same license as the original NupizLang project.

## Acknowledgments

- **Original Project**: [Lemon-chad/NupizLang](https://github.com/Lemon-chad/NupizLang)
- **Contributors**: All the amazing people who help improve AiNupiz
- **Community**: Everyone who uses and supports the project

Thank you for contributing to AiNupiz! 🚀
