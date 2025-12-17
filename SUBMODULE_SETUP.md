# Fixing Private Submodule Access

Your repository has two submodules, and `protocol-armory` is private. Here are solutions to access it:

## Solution 1: Use SSH (Recommended if you have SSH keys set up)

1. **Set up SSH keys** (if not already done):
   ```bash
   ssh-keygen -t ed25519 -C "your_email@example.com"
   cat ~/.ssh/id_ed25519.pub
   # Add this key to your GitHub account: Settings > SSH and GPG keys
   ```

2. **Test SSH connection**:
   ```bash
   ssh -T git@github.com
   ```

3. **Update submodule URL to SSH**:
   ```bash
   git config submodule.src/third-party/protocol-armory.url git@github.com:map-and-stream/protocol-armory.git
   ```

4. **Update .gitmodules** (change HTTPS to SSH):
   ```bash
   # Edit .gitmodules and change:
   # url = https://github.com/map-and-stream/protocol-armory.git
   # to:
   # url = git@github.com:map-and-stream/protocol-armory.git
   ```

5. **Initialize and clone the submodule**:
   ```bash
   git submodule sync
   git submodule update --init --recursive src/third-party/protocol-armory
   ```

## Solution 2: Use HTTPS with Personal Access Token (PAT)

1. **Create a GitHub Personal Access Token**:
   - Go to GitHub: Settings > Developer settings > Personal access tokens > Tokens (classic)
   - Generate new token with `repo` scope
   - Copy the token

2. **Configure Git to use token**:
   ```bash
   # Option A: Store credentials in Git credential helper
   git config --global credential.helper store
   # When prompted during clone, use your GitHub username and the PAT as password
   
   # Option B: Use token directly in URL (less secure)
   git config submodule.src/third-party/protocol-armory.url https://YOUR_TOKEN@github.com/map-and-stream/protocol-armory.git
   ```

3. **Initialize the submodule**:
   ```bash
   git submodule sync
   git submodule update --init --recursive src/third-party/protocol-armory
   ```

## Solution 3: Manual Clone and Register

If the above don't work, manually clone and register:

```bash
# Clone the private repository manually
git clone https://github.com/map-and-stream/protocol-armory.git src/third-party/protocol-armory

# Register it as a submodule
cd src/third-party/protocol-armory
git checkout <commit-hash>  # Get hash from parent repo: git ls-tree HEAD src/third-party/protocol-armory
cd ../../..
git submodule sync
```

## Verify Installation

After any solution, verify with:
```bash
git submodule status
ls -la src/third-party/
```

Both submodules should appear in the status output.



