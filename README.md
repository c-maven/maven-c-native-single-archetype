# Maven C Native Single Archetype

Maven 通过 `native-maven-plugin` 构建 C 程序，目录结构保留类似 Java Maven 项目的 `src/main` 和 `src/test` 分层。

生成后的项目结构：

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

## 1. 前置工具

确保以下命令已经进入 `PATH`：

```bash
mvn -version
clang --version
```

Windows 下推荐使用 MSYS2/MinGW 的 `clang` 或 `gcc`。

项目内置 `.mvn/jvm.config`，用于避免 Maven 3.x 在 JDK 24+ 上由 Guice 触发的 `sun.misc.Unsafe::staticFieldBase` warning。

## 2. 安装 Archetype

在本项目根目录执行：

```bash
mvn clean install
```

它会安装坐标：

```text
com.weiweixu.archetypes:maven-c-native-single-archetype:1.0.0
```

## 3. 发布到 GitHub Packages

本仓库已经配置 GitHub Packages 发布地址：

```text
https://maven.pkg.github.com/c-maven/maven-c-native-single-archetype
```

发布方式：

- 在 GitHub 上创建并发布 Release。
- 或手动运行 Actions 里的 `Publish Maven Package` workflow。

发布使用仓库自带的 `GITHUB_TOKEN`，不需要额外配置 secret。注意同一个 Maven 版本不能重复发布；如果 `1.0.0` 已经发布过，需要先升级 `pom.xml` 中的 `<version>`。

如果要在本机手动发布，需要在 `~/.m2/settings.xml` 配置 `github` 这个 server：

```xml
<settings>
  <servers>
    <server>
      <id>github</id>
      <username>你的 GitHub 用户名</username>
      <password>有 write:packages 权限的 token</password>
    </server>
  </servers>
</settings>
```

然后执行：

```bash
mvn clean deploy
```

## 4. 从 GitHub Packages 生成一个 C 项目

使用 GitHub Packages 里的 archetype 前，需要在 Maven settings 中配置读取仓库。GitHub Packages 对 Maven 读取通常也需要认证；token 至少需要 `read:packages` 权限。

```xml
<settings>
  <servers>
    <server>
      <id>github</id>
      <username>你的 GitHub 用户名</username>
      <password>有 read:packages 权限的 token</password>
    </server>
  </servers>
  <profiles>
    <profile>
      <id>github-packages</id>
      <repositories>
        <repository>
          <id>github</id>
          <url>https://maven.pkg.github.com/c-maven/maven-c-native-single-archetype</url>
        </repository>
      </repositories>
    </profile>
  </profiles>
  <activeProfiles>
    <activeProfile>github-packages</activeProfile>
  </activeProfiles>
</settings>
```

生成项目：

```bash
mvn archetype:generate \
  -DarchetypeGroupId=com.weiweixu.archetypes \
  -DarchetypeArtifactId=maven-c-native-single-archetype \
  -DarchetypeVersion=1.0.0 \
  -DgroupId=com.example.c \
  -DartifactId=hello-c \
  -Dversion=1.0.0-SNAPSHOT \
  -DinteractiveMode=false
```

## 5. 编译、运行

```bash
cd hello-c
mvn clean package
```

Windows 下构建产物预计位于：

```text
target/hello-c.exe
```

运行：

```bash
./target/hello-c.exe
```

临时更换 C 编译器：

```bash
mvn clean package -Dc.compiler=gcc
```

## 6. IDEA / CLion

直接用 IntelliJ IDEA 打开项目根目录或 `pom.xml`，按 Maven 项目导入。

`CMakeLists.txt` 保留给 CLion/CMake 做代码模型、运行和调试；Maven 正式构建使用 `native-maven-plugin`。
