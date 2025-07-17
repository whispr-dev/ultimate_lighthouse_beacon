

🚀 Proper tmux workflow:
Step 1: Detach from tmux (keep it running)

# While in your tmux session, press:
Ctrl+B then d

# You should see: [detached (from session 0)]Step 2: Close terminal safely

Now you can close your SSH terminal window - the lighthouse will keep running! 🏰
Step 3: Reconnect later

# SSH back in
ssh wofl@161.35.248.233

# Reattach to your running tmux session
tmux attach

# Or if you have multiple sessions:
tmux list-sessions
tmux attach -t 0

🔍 Check if it's still running:

# Before detaching, you can verify processes are running
ps aux | grep ultimate
# Should show your beacon and listener processes

# After reconnecting, check again
ps aux | grep ultimate

🎯 Pro tip:

# You can also name your tmux session for easier management
tmux new -s lighthouse
# Then later: tmux attach -t lighthouse

🏰 Your lighthouse will survive:

✅ Terminal disconnections
✅ SSH session drops
✅ Network hiccups
✅ You closing your laptop

Press Ctrl+B then d to detach, then close away, husklyfren! Your lighthouse will keep beaming! 🚀⚡
