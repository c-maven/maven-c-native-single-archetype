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
io.github.aurorxa:maven-c-native-single-archetype:0.0.1
```

## 3. 发布到 GitHub Packages

本仓库已经配置 GitHub Packages 发布地址：

```text
https://maven.pkg.github.com/c-maven/maven-c-native-single-archetype
```

发布方式：

- 在 GitHub 上创建并发布 Release。
- 或手动运行 Actions 里的 `Publish Maven Package` workflow。

发布使用仓库自带的 `GITHUB_TOKEN`，不需要额外配置 secret。注意同一个 Maven 版本不能重复发布；如果 `0.0.1` 已经发布过，需要先升级 `pom.xml` 中的 `<version>`。

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
mvn clean deploy -Pgithub-packages
```

## 4. 从 GitHub Packages 生成一个 C 项目

GitHub Packages 的 Maven 包可以设置成公开可见，但通过 Maven 命令下载公共包时仍然需要认证。使用 GitHub Packages 里的 archetype 前，需要在 Maven settings 中配置读取仓库；token 至少需要 `read:packages` 权限。

如果希望任何人不配置 token 就能直接使用，需要发布到 Maven Central 这类公共 Maven 仓库，而不是 GitHub Packages。

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

通过 GitHub Packages 生成项目：

```bash
mvn archetype:generate \
  -DarchetypeGroupId=io.github.aurorxa \
  -DarchetypeArtifactId=maven-c-native-single-archetype \
  -DarchetypeVersion=0.0.1 \
  -DgroupId=com.example.c \
  -DartifactId=hello-c \
  -Dversion=1.0.0-SNAPSHOT \
  -DinteractiveMode=false
```

## 5. 发布到 Maven Central

Maven Central 发布后，其他人不需要 GitHub token 就能直接使用。

发布前需要准备：

- 在 Sonatype Central Portal 注册账号。
- 创建并验证 namespace。当前坐标是 `io.github.aurorxa`，需要在 Central Portal 中验证这个 GitHub namespace。
- 生成 GPG 签名密钥，并把公钥发布到 keyserver。
- 在 Central Portal 生成 User Token。

项目 POM 已经配置：

- Apache-2.0 license 元数据。
- developer 和 SCM 元数据。
- `central-release` profile。
- `-sources.jar` 和 `-javadoc.jar` 附件包。
- GPG 签名。
- Sonatype Central Portal 发布插件。

生成并发布 GPG 公钥。交互式生成：

```bash
gpg --full-generate-key
gpg --list-secret-keys --keyid-format=long
gpg --keyserver keyserver.ubuntu.com --send-keys <KEY_ID>
```

如果不想交互确认，可以使用 batch 配置文件。先创建一个临时文件 `gpg-key.conf`：

```text
Key-Type: RSA
Key-Length: 4096
Subkey-Type: RSA
Subkey-Length: 4096
Name-Real: Aurorxa
Name-Email: your-email@example.com
Expire-Date: 0
Passphrase: your-gpg-passphrase
%commit
```

然后执行：

```bash
gpg --batch --generate-key gpg-key.conf
gpg --list-secret-keys --keyid-format=long
gpg --keyserver keyserver.ubuntu.com --send-keys <KEY_ID>
```

`gpg-key.conf` 里包含私钥密码，生成完成后应立即删除，不要提交到仓库。

如果在 Windows、Git Bash 或 MSYS2 中看到类似错误：

```text
gpg: error running '/usr/lib/gnupg/keyboxd': probably not installed
gpg: error opening key DB: No Keybox daemon running
```

说明当前 GnuPG 安装不完整，或 `~/.gnupg/common.conf` 启用了 `use-keyboxd` 但本机没有可用的 `keyboxd`。推荐使用 Gpg4win，并在 PowerShell 中执行上述 `gpg` 命令；如果使用 MSYS2，需要先安装完整 GnuPG：

```bash
pacman -Syu
pacman -S gnupg
```

也可以检查并移除 `~/.gnupg/common.conf` 里的 `use-keyboxd` 配置后重试。

导出给 GitHub Actions 使用的 GPG 私钥：

```bash
gpg --armor --export-secret-keys <KEY_ID>
```

把输出的完整内容保存到 GitHub 仓库的 `Settings` -> `Secrets and variables` -> `Actions` 中。

需要配置这些 Repository secrets：

```text
CENTRAL_USERNAME     Central Portal User Token 的 username
CENTRAL_PASSWORD     Central Portal User Token 的 password
GPG_PRIVATE_KEY      gpg --armor --export-secret-keys <KEY_ID> 的完整输出
GPG_PASSPHRASE       GPG 私钥密码
```

不要把 Central token、GPG 私钥或 GPG 密码写进仓库。已经暴露过的 token 应立即在 Central Portal 中撤销，并重新生成后再放入 GitHub Secrets。

本机发布时，`~/.m2/settings.xml` 需要配置 Central Portal token：

```xml
<settings>
  <servers>
    <server>
      <id>central</id>
      <username>Central Portal 用户 token 的 username</username>
      <password>Central Portal 用户 token 的 password</password>
    </server>
  </servers>
</settings>
```

上传并等待 Central Portal 校验：

```bash
mvn clean deploy -Pcentral-release
```

当前 POM 设置了 `autoPublish=false`。校验通过后，需要到 Central Portal 中确认并点击 Publish；这样可以避免错误版本被自动公开发布。

如果 GPG 私钥设置了密码，并且本机没有弹出输入框，可以传入：

```bash
mvn clean deploy -Pcentral-release -Dgpg.passphrase=<你的 GPG 私钥密码>
```

GitHub Actions 自动发布：

- 手动运行 `Publish Maven Central` workflow。
- 或发布 GitHub Release，自动触发 `Publish Maven Central` workflow。

流水线会执行：

```bash
mvn --batch-mode clean deploy -Pcentral-release -Dcentral.autoPublish=true -Dcentral.waitUntil=published
```

发布成功并同步后，别人可以直接生成项目：

```bash
mvn archetype:generate \
  -DarchetypeGroupId=io.github.aurorxa \
  -DarchetypeArtifactId=maven-c-native-single-archetype \
  -DarchetypeVersion=0.0.1 \
  -DgroupId=com.example.c \
  -DartifactId=hello-c \
  -Dversion=1.0.0-SNAPSHOT \
  -DinteractiveMode=false
```

## 6. 编译、运行

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

## 7. IDEA / CLion

直接用 IntelliJ IDEA 打开项目根目录或 `pom.xml`，按 Maven 项目导入。

`CMakeLists.txt` 保留给 CLion/CMake 做代码模型、运行和调试；Maven 正式构建使用 `native-maven-plugin`。
