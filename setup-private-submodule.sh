#!/bin/bash

# Script to set up private submodule access
# Choose your authentication method and run the appropriate section

echo "Setting up private submodule: protocol-armory"
echo ""

# Check if submodule directory already exists
if [ -d "src/third-party/protocol-armory" ]; then
    echo "Submodule directory already exists. Skipping clone."
    exit 0
fi

# Method 1: Try SSH first (if SSH keys are configured)
echo "Method 1: Attempting SSH clone..."
if git clone git@github.com:map-and-stream/protocol-armory.git src/third-party/protocol-armory 2>/dev/null; then
    echo "✓ Successfully cloned via SSH"
    exit 0
fi

echo "SSH clone failed, trying HTTPS..."

# Method 2: HTTPS (will prompt for credentials or use stored credentials)
if git clone https://github.com/map-and-stream/protocol-armory.git src/third-party/protocol-armory; then
    echo "✓ Successfully cloned via HTTPS"
    exit 0
fi

echo ""
echo "✗ Clone failed. Please ensure:"
echo "  1. You have access to the repository"
echo "  2. Your SSH keys are set up (for SSH method)"
echo "  3. Your Git credentials are configured (for HTTPS method)"
echo ""
echo "For HTTPS: Use Personal Access Token as password when prompted"
echo "For SSH: Run 'ssh-add ~/.ssh/id_ed25519' (or your key file)"
exit 1

