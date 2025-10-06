import json
import os

FILE_NAME = "students.json"

def load_data():
    """Carrega dados do arquivo JSON, se existir e for válido."""
    if os.path.exists(FILE_NAME):
        try:
            with open(FILE_NAME, "r", encoding="utf-8") as f:
                content = f.read().strip()
                if not content:
                    return {}  # Arquivo vazio
                return json.loads(content)
        except json.JSONDecodeError:
            print("⚠️ Arquivo JSON corrompido. Será recriado.")
            return {}
    return {}

def save_data(data):
    """Salva dados no arquivo JSON."""
    with open(FILE_NAME, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)
