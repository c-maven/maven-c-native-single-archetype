# ${artifactId}

这是由 `maven-c-native-single-archetype` 生成的 C23 项目。项目结构参考 `Aurorxa/maven-c-demo`，Maven 通过 `native-maven-plugin` 完成正式构建。

## 项目结构

```text
${artifactId}/
├── pom.xml
├── CMakeLists.txt
├── .clang-format
├── .gitignore
└── src/
    ├── main/
    │   ├── c/
    │   │   ├── hello.c
    │   │   └── main.c
    │   ├── include/
    │   │   └── hello.h
    │   └── resources/
    │       └── .gitkeep
    └── test/
        ├── c/
        │   └── test_hello.c
        └── resources/
            └── .gitkeep
```

## Maven 命令

```bash
mvn clean package
```

Windows 下构建产物预计位于：

```text
target/${artifactId}.exe
```

运行：

```bash
./target/${artifactId}.exe
```

临时更换 C 编译器：

```bash
mvn clean package -Dc.compiler=gcc
```

项目内置 `.mvn/jvm.config`，用于避免 Maven 3.x 在 JDK 24+ 上由 Guice 触发的 `sun.misc.Unsafe::staticFieldBase` warning。

## CLion

直接打开项目根目录的 `CMakeLists.txt`。Maven 用于正式构建，CLion/CMake 用于日常编码、运行和调试。
