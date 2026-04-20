#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <stdexcept>
#include <format>   // C++20 — esto es nuevo

using namespace std;
class Entity {
private:
    int x;
    int y;
    double vida;
    string nombre;
    int nivel;
    string recursos;
public:
    Entity(string nombre) {
        this->nombre = nombre;
        this->vida = 100;
        this->nivel = 1;
        this -> x = 0;
        this -> y = 0;
        this-> recursos = "";
    }
    void levelUp() {
        this->nivel++;
        cout << format("[Entity] {} subio al nivel {}!\n", nombre, nivel);
    }
    void addRecurso(const string& r) {
        if (!recursos.empty()) {
            recursos += r ;

        }

    }
    void move(int x , int y) {
        this->x = x;
        this->y = y;
    }
    void heal(double v) {
        this->vida += v;
    }
    void damage(double d) {
        this->vida = max(0.0,vida - d);
    }
    void reset() {
        this->vida = 100;
        this->x = 0;
        this->y = 0;
        this->nivel = 1;

        this->recursos = "";
    }
    string snapshot() const {
        return format("vida={:.1f} pos=({},{}) nivel={}", vida, x, y, nivel);
    }
    void status() {
        cout << "=== Estado ===" << endl;
        cout << format("Nombre : {}\n", nombre);
        cout << format("Vida   : {:.1f}\n", vida);
        cout << format("Pos    : ({}, {})\n", x, y);
        cout << format("Nivel  : {}\n", nivel);
        cout << format("Recursos: {}\n", recursos.empty() ? "(ninguno)" : recursos);
        cout << "==============" << endl;
    }

};
using Command = function<void(const list<string>&)>;
class CommandCenter {
private:
    map<string,Command> commands;
    list<string> historial;
    Entity & entity;
    map<string, list<pair<string, list<string>>>> macros; //guarda mas que tod una pasosx
public:
    CommandCenter(Entity&e): entity(e){}
    void registerCommand(const string &nombre, Command com) {
        commands[nombre] = com;
    }
    void execute(const string& nombre, const list<string>& args) {

        map<string, Command>::iterator it = commands.find(nombre);
        if (it != commands.end()) {

            string antes = entity.snapshot();
            it->second(args);

            string despues = entity.snapshot();

            historial.push_back(
                format("cmd='{}' | antes: [{}] -> despues: [{}]", nombre, antes, despues)
            );
        } else {

            cout << format("[CommandCenter] Error: comando '{}' no encontrado.\n", nombre);
        }
    }
    void deleteCommand(const string& nombre) {
        map<string, Command>::iterator it = commands.find(nombre);
        if (it != commands.end()) {
            commands.erase(it);

            cout << format("[CommandCenter] Comando '{}' eliminado.\n", nombre);
        } else {

            cout << format("[CommandCenter] No se puede borrar '{}': no existe.\n", nombre);
        }
    }
    void registerMacro(const string & nombre , const list<pair<string, list <string>>> pasos) {
        macros[nombre] = pasos;

    }
    void executeMacro(const string& nombre) {

        map<string, list<pair<string, list<string>>>>::iterator macroIt = macros.find(nombre);
        if (macroIt == macros.end()) {
            cout << format("[CommandCenter] Macro '{}' no encontrado.\n", nombre);
            return;
        }
        cout << format("--- Ejecutando macro: {} ---\n", nombre);

        list<pair<string, list<string>>>::iterator paso = macroIt->second.begin();
        while (paso != macroIt->second.end()) {

            map<string, Command>::iterator cmdIt = commands.find(paso->first);
            if (cmdIt == commands.end()) {
                cout << format("[CommandCenter] Macro detenido: comando '{}' no existe.\n", paso->first);
                return;
            }
            execute(paso->first, paso->second);
            ++paso;
        }
        cout << format("--- Macro '{}' completado ---\n", nombre);
    }
    void printHistorial() const {
        cout << "\n=== Historial de ejecucion ===" << endl;
        list<string>::const_iterator it = historial.begin();
        int i = 1;
        while (it != historial.end()) {
            cout << format("  {}. {}\n", i++, *it);
            ++it;
        }
        cout << "==============================\n" << endl;
    }

};
void cmdStatus(Entity& e, const list<string>& args) {
    e.status();
}
void cmdLevelUp(Entity& e, const list<string>& args) {
    (void)args;
    e.levelUp();
}

class ResetCommand {
private:
    Entity& entity;
    int contador;  // estado interno propio
public:
    ResetCommand(Entity& e) : entity(e), contador(0) {}

    void operator()(const list<string>& args) {
        entity.reset();
        contador ++;
        cout << format("[ResetCommand] Reset completado {} vece(s).\n", contador);
    }
};
class LimitedHealCommand {
private:
    Entity& entity;
    int usos;
    int limite;
public:
    LimitedHealCommand(Entity& e, int lim) : entity(e), usos(0), limite(lim) {}

    void operator()(const list<string>& args) {
        if (usos >= limite) {
            cout << format("[LimitedHeal] Limite de {} usos alcanzado.\n", limite);
            return;
        }

        if (args.empty()) {
            cout << "[LimitedHeal] Error: se requiere 1 argumento.\n";
            return;
        }
        try {
            double v = stod(args.front());
            if (v <= 0) { cout << "[LimitedHeal] Error: valor debe ser positivo.\n"; return; }
            entity.heal(v);
            usos++;
            cout << format("[LimitedHeal] Curado {:.1f} hp. Uso {}/{}.\n", v, usos, limite);
        } catch (const invalid_argument&) {

            cout << "[LimitedHeal] Error: argumento no es un numero valido.\n";
        }
    }
};
int main() {
    Entity e("Goku");
    ResetCommand       resetCmd(e);
    LimitedHealCommand limitedHeal(e, 2);
    CommandCenter      center(e);

    center.registerCommand("status",  [&e](const list<string>& args) { cmdStatus(e, args);  });
    center.registerCommand("levelup", [&e](const list<string>& args) { cmdLevelUp(e, args); });

    center.registerCommand("heal", [&e](const list<string>& args) {
        if (args.empty()) { cout << "[heal] Error: se requiere 1 argumento.\n"; return; }
        try {
            double n = stod(args.front());
            if (n <= 0) { cout << "[heal] Error: valor debe ser positivo.\n"; return; }
            e.heal(n);
        } catch (const invalid_argument&) {
            cout << "[heal] Error: argumento no es un numero valido.\n";
        }
    });

    center.registerCommand("damage", [&e](const list<string>& args) {
        if (args.empty()) { cout << "[damage] Error: se requiere 1 argumento.\n"; return; }
        try {
            double d = stod(args.front());
            if (d <= 0) { cout << "[damage] Error: valor debe ser positivo.\n"; return; }
            e.damage(d);
        } catch (const invalid_argument&) {
            cout << "[damage] Error: argumento no es un numero valido.\n";
        }
    });

    center.registerCommand("move", [&e](const list<string>& args) {
        if (args.size() < 2) { cout << "[move] Error: se requieren 2 argumentos (x y).\n"; return; }
        try {
            list<string>::const_iterator it = args.begin();
            int x = stoi(*it); ++it;
            int y = stoi(*it);
            e.move(x, y);
        } catch (const invalid_argument&) {
            cout << "[move] Error: argumentos deben ser enteros.\n";
        }
    });

    center.registerCommand("addrecurso", [&e](const list<string>& args) {
        if (args.empty()) { cout << "[addrecurso] Error: se requiere 1 argumento.\n"; return; }
        e.addRecurso(args.front());
    });

    center.registerCommand("reset",       resetCmd);
    center.registerCommand("limitedheal", limitedHeal);

    center.registerMacro("heal_and_status",      { {"heal", {"20"}}, {"status", {}} });
    center.registerMacro("full_damage_sequence", { {"damage", {"30"}}, {"damage", {"20"}}, {"status", {}} });
    center.registerMacro("reset_and_levelup",    { {"reset", {}}, {"levelup", {}}, {"levelup", {}}, {"status", {}} });
    center.registerMacro("prepare_battle",       { {"heal", {"50"}}, {"addrecurso", {"espada"}}, {"move", {"10", "20"}}, {"status", {}} });

    list<string> noArgs;

    cout << "\n====== DEMO: Funciones libres ======\n";
    center.execute("status",  noArgs);
    center.execute("levelup", noArgs);
    center.execute("levelup", noArgs);

    cout << "\n====== DEMO: Lambdas ======\n";
    center.execute("heal",       {"15"});
    center.execute("damage",     {"40"});
    center.execute("move",       {"5", "10"});
    center.execute("addrecurso", {"pocion"});
    center.execute("status",     noArgs);

    cout << "\n====== DEMO: Functor ResetCommand ======\n";
    center.execute("reset", noArgs);
    center.execute("heal",  {"10"});
    center.execute("reset", noArgs);
    center.execute("reset", noArgs);

    cout << "\n====== DEMO: Functor LimitedHeal (limite=2) ======\n";
    center.execute("limitedheal", {"30"});
    center.execute("limitedheal", {"30"});
    center.execute("limitedheal", {"30"});

    cout << "\n====== DEMO: Comandos invalidos ======\n";
    center.execute("comer",  noArgs);
    center.execute("heal",   noArgs);
    center.execute("damage", {"abc"});
    center.execute("move",   {"5"});

    cout << "\n====== DEMO: Eliminacion dinamica ======\n";
    center.deleteCommand("damage");
    center.execute("damage",  {"10"});
    center.deleteCommand("damage");

    cout << "\n====== DEMO: Macros ======\n";
    center.executeMacro("heal_and_status");
    center.executeMacro("full_damage_sequence");
    center.executeMacro("reset_and_levelup");
    center.executeMacro("prepare_battle");
    center.executeMacro("macro_inexistente");

    center.printHistorial();

    return 0;
}