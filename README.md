## Repository Introduction
Welcome to the repository for all Software implemented for RC Aircraft Team #1: The One Plane! This here contains the separated files for each aspect of our Software, and a completed file for optimization.

This GitHub repository is utilized to implement version control, cloud back-ups, easier collaboration, edit traceability, and professional documentation for our team.

## Adding/Pulling Software Code
To add your Software code to this repository, please follow these steps (Link: https://docs.soldered.com/arduino/GitHub/):
### Prerequisites:
Download Git (GitBash).
Sign-Up for GitHub.

### Step-by-Step GitHub Set-Up For Not-Yet Added Arduino Projects:
1. Navigate to Your Arduino Project Folder in File Explorer (ex. _Documents/Arduino/exampleProject/_)
1.5. Navigate to your Project Folder in Git (using command **cd**)
2. Initialize Git by running command: **git init**. This turns the project folder into a Git repository)
3. Check what will be tracked/untracked by running command: **git status**
	4. (Optional) Add a _.gitignore_ file within File Explorer to determine which files shall not be put into GitHub
5. Add all files to the repository by running command: **git add .**
5.5. Save the current version of your project by running command: **git commit -m "[Commit Info]"**
	For example: "_git commit -m "Implemented 'Enter Sandman' Into Buzzer List of Songs"_"
7. Link your local repository to GitHub by running the following commands:
	**git remote add origin [GitHub HTTPS Link] (main)
	git branch -M main
	git push -u origin main**

### Typical Workflow:
When you make changes in Arduino that you wish to add to GitHub, commit and push your changes by running the commands:
1. **git pull**
2. **git add .**
3. **git commit -m "[Commit Info]"**
4. **git push**

### To Pull GitHub Code:
**git clone [GitHub HTTPS Link].git**
1. Open cloned folder in Arduino IDE and continue development!

If you have any questions, let me know!
