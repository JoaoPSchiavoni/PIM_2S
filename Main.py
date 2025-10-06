import coreFunction
import fileService

# -------------------- MAIN MENU --------------------

def main_menu():
    data = fileService.load_data()

    while True:
        print("\n========== MENU PRINCIPAL ==========")
        print("1. Adicionar Aluno")
        print("2. Inserir Notas")
        print("3. Editar Notas")
        print("4. Visualizar Notas")
        print("5. Sair")
        print("====================================")

        option = input("Escolha uma opção: ").strip()

        if option == "1":
            coreFunction.add_student(data)
        elif option == "2":
            coreFunction.insert_grades(data)
        elif option == "3":
            coreFunction.edit_grades(data)
        elif option == "4":
            coreFunction.view_grades(data)
        elif option == "5":
            print("👋 Encerrando o programa... Até logo!")
            break
        else:
            print("❌ Opção inválida. Tente novamente.")


if __name__ == "__main__":
    main_menu()
