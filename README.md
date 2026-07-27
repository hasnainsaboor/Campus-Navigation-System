# 🗺️ Campus Navigation System

An interactive **Campus Navigation System** designed to help users navigate a university campus efficiently using graph algorithms. The application combines **C++**, **Python (Flask)**, **SQLite**, and modern web technologies to provide a seamless and responsive navigation experience.

## 🚀 Features

- Interactive campus map
- Shortest path calculation using **Dijkstra's Algorithm**
- Graph traversal using **Breadth-First Search (BFS)**
- Secure Admin Login with password hashing using **hashlib**
- Building and path management
- Building information system
- RESTful API built with Flask
- SQLite database integration
- Responsive user interface built with HTML, CSS, and JavaScript

---

## 🛠️ Tech Stack

### Backend
- C++
- Python
- Flask

### Frontend
- HTML5
- CSS3
- JavaScript

### Database
- SQLite
- Python `sqlite3`

### Security
- Password hashing using `hashlib`

---

## 🧠 Algorithms Used

### Dijkstra's Algorithm
Calculates the shortest path between two campus locations using weighted graph traversal.

### Breadth-First Search (BFS)
Traverses the campus graph level by level for route exploration and visualization.

---

## 📂 Project Structure

```text
Campus-Navigation-System/
│
├── assets/
├── data/
│   └── campus.db
├── include/
├── src/
├── sqlite/
├── static/
├── templates/
├── server.py
├── campus_navigator.html
├── README.md
├── requirements.txt
└── .gitignore
```

---

## ⚙️ Installation

### 1. Clone the repository

```bash
git clone https://github.com/hasnainsaboor/Campus-Navigation-System.git
cd Campus-Navigation-System
```

### 2. Install dependencies

```bash
pip install -r requirements.txt
```

Or install Flask manually:

```bash
pip install flask
```

### 3. Run the application

```bash
python server.py
```

### 4. Open in your browser

```
http://localhost:5000
```

---

## 🗄️ Database

The project uses **SQLite** as its database management system. Python's built-in `sqlite3` module is used to interact with the database and manage:

- Buildings
- Paths
- Building Information
- Administrator Credentials

---

## 🔒 Security

Administrator passwords are securely hashed using Python's `hashlib` library before being stored in the database, ensuring that plaintext passwords are never saved.

---

## 🎯 Learning Outcomes

This project demonstrates practical experience with:

- Graph Theory
- Data Structures & Algorithms
- Full-Stack Web Development
- REST API Development
- Database Design
- Authentication & Security
- Backend Integration
- Software Engineering Principles

---

## 🔮 Future Improvements

- Live location support
- Mobile-responsive design
- Route animation
- Search and filtering
- Estimated travel time
- Multi-user authentication
- Interactive building details

---

## 👨‍💻 Author

**Hasnain Saboor**

BS Computer Science Student  
University of Karachi (UBIT)

- GitHub: https://github.com/hasnainsaboor
