import uuid
import sys
sys.path.append('/Users/mugen/Desktop/PIM/PIM_2S')
import fileService


# -------------------- CORE FUNCTIONS --------------------

def add_student(data):
    name = input("Digite o nome do aluno: ").strip()
    if not name:
        print("❌ Nome inválido!")
        return

    student_id = str(uuid.uuid4())[:8]
    ra = str(len(data) + 1).zfill(5)

    subjects = [
        "Matemática", "Biologia", "Física", "Português", "Inglês",
        "Filosofia", "Sociologia", "Química", "Geografia",
        "Educação Física", "História"
    ]

    data[student_id] = {
        "name": name,
        "RA": ra,
        "grades": {subject: [] for subject in subjects}
    }

    fileService.save_data(data)
    print(f"✅ Aluno '{name}' adicionado com sucesso! (RA: {ra}, ID: {student_id})")


def insert_grades(data):
    ra = input("Digite o RA do aluno: ").strip()
    student = next((s for s in data.values() if s["RA"] == ra), None)

    if not student:
        print("❌ Aluno não encontrado!")
        return

    subjects = list(student["grades"].keys())

    print("\n📘 Disciplinas disponíveis:")
    for i, subject in enumerate(subjects, 1):
        print(f"{i}. {subject}")

    try:
        choice = int(input("Escolha a disciplina (número): "))
        subject = subjects[choice - 1]
    except (ValueError, IndexError):
        print("❌ Opção inválida.")
        return

    current_grades = student["grades"][subject]
    if len(current_grades) >= 4:
        print("⚠️ Essa disciplina já possui 4 notas. Não é possível adicionar mais.")
        return

    try:
        new_grades = [float(g) for g in input("Digite as notas separadas por espaço: ").split()]
    except ValueError:
        print("❌ Entrada inválida. Digite apenas números.")
        return

    if len(current_grades) + len(new_grades) > 4:
        print("⚠️ Só é permitido no máximo 4 notas por disciplina.")
        return

    student["grades"][subject].extend(new_grades)
    fileService.save_data(data)
    print(f"✅ Notas adicionadas para {student['name']} em {subject}!")


def edit_grades(data):
    ra = input("Digite o RA do aluno: ").strip()
    student = next((s for s in data.values() if s["RA"] == ra), None)

    if not student:
        print("❌ Aluno não encontrado!")
        return

    print(f"\n📄 Notas de {student['name']}:\n")
    for subject, grades in student["grades"].items():
        avg = (sum(grades) / len(grades)) if grades else 0
        status = "Aprovado ✅" if avg >= 6.5 else "Reprovado ❌"
        print(f"{subject}: {grades} (Média: {avg:.2f}) - {status if grades else 'Sem notas'}")

    subject = input("\nQual disciplina deseja editar? ").strip().title()

    if subject not in student["grades"]:
        print("❌ Disciplina inválida.")
        return

    grades = student["grades"][subject]
    if not grades:
        print("⚠️ Nenhuma nota encontrada para essa disciplina.")
        return

    print(f"Notas atuais em {subject}: {grades}")
    try:
        index = int(input("Digite o índice da nota que deseja editar (0, 1, 2, 3): "))
        new_grade = float(input("Digite a nova nota: "))
        grades[index] = new_grade
        fileService.save_data(data)
        print("✅ Nota atualizada com sucesso!")
    except (ValueError, IndexError):
        print("❌ Índice ou valor inválido.")


def view_grades(data):
    if not data:
        print("⚠️ Nenhum aluno cadastrado.")
        return

    print("\n══════════════════════════════════════════════════════")
    print("📚 RELATÓRIO DE NOTAS DOS ALUNOS")
    print("══════════════════════════════════════════════════════\n")

    # Dicionário auxiliar para cálculo das médias por disciplina
    subject_totals = {}
    subject_counts = {}

    for student_id, student in data.items():
        print(f"👤 Aluno: {student['name']}")
        print(f"🆔 ID: {student_id}")
        print(f"🎓 RA: {student['RA']}")
        print("──────────────────────────────────────────")

        for subject, grades in student["grades"].items():
            if grades:
                avg = sum(grades) / len(grades)
                status = "Aprovado ✅" if avg >= 6.5 else "Reprovado ❌"
                print(f"📘 {subject:<20} -> Notas: {grades} | Média: {avg:>5.2f} | {status}")

                # Soma para média geral da matéria
                if subject not in subject_totals:
                    subject_totals[subject] = 0
                    subject_counts[subject] = 0
                subject_totals[subject] += avg
                subject_counts[subject] += 1
            else:
                print(f"📗 {subject:<20} -> Sem notas cadastradas")

        print("══════════════════════════════════════════════════════\n")

    # Mostra médias por disciplina (geral da turma)
    if subject_totals:
        print("📊 MÉDIAS GERAIS POR DISCIPLINA")
        print("══════════════════════════════════════════════════════")
        for subject, total in subject_totals.items():
            count = subject_counts.get(subject, 1)
            subject_avg = total / count
            status = "Aprovado ✅" if subject_avg >= 6.5 else "Reprovado ❌"
            print(f"{subject:<25} -> Média da turma: {subject_avg:>5.2f} | {status}")
        print("══════════════════════════════════════════════════════")
    else:
        print("🟡 Nenhuma nota registrada ainda.")