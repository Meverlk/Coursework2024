from flask import Flask, render_template, request, jsonify
import requests

app = Flask(__name__)

SERVER_URL = "http://localhost:8080"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/search', methods=['GET'])
def search():
    query = request.args.get('query')
    if query:
        # Відправка запиту на сервер
        response = requests.get(f"{SERVER_URL}/?query={query}")
        if response.status_code == 200:
            return jsonify(response.json())
        else:
            return jsonify({"error": "Server error"}), 500
    return jsonify({"error": "No query provided"}), 400

if __name__ == '__main__':
    app.run(debug=True)
