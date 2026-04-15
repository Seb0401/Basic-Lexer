#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

// Lista de palabras reservadas del lenguaje
vector<string> keywords={"#include","using","namespace","std","int","float","return","if","else","void"};

// Verifica si una cadena coincide con alguna palabra reservada
bool esKeyword(string palabra){
    for(int i=0;i<keywords.size();i++){
        if(palabra==keywords[i]){
            return true;
        }
    }
    return false;
}

// Estructura para guardar el texto (lexema) y su categoría (tipo)
struct Token{
    string lexema;
    string tipo;
};

int main(){
    // Vectores para almacenar los resultados del análisis
    vector<Token> listaTokens;
    vector<string> tablaSimbolos;

    // Intenta abrir el archivo fuente
    ifstream archivo("codigo.txt");
    
    // Verifica que el archivo exista y se pueda leer
    if(!archivo.is_open()){
        cout<<"Error: No se pudo abrir el archivo 'codigo.txt'."<<endl;
        return 1;
    }

    // Variables de control de línea y estado de errores
    string linea;
    int numLinea=0;
    bool encontroErrores=false;

    // Bucle principal: lee el archivo línea por línea
    while(getline(archivo,linea)){
        numLinea++;
        int i=0;
        int n=linea.length();

        // Bucle secundario: recorre cada carácter de la línea actual
        while(i<n){
            char c=linea[i];

            // Salta los espacios en blanco
            if(isspace(c)){
                i++;
                continue;
            }

            // EXPLICACIÓN: c=='/' verifica si el caracter actual es un slash.
            // i+1<n asegura que no estemos en el último caracter de la línea para no leer memoria que no existe (evita fallos de segmentación).
            // linea[i+1]=='/' revisa si el *siguiente* caracter también es slash.
            if(c=='/'&&i+1<n&&linea[i+1]=='/'){
                break; // Rompe la lectura de esta línea porque el resto es comentario
            }

            // EXPLICACIÓN: Extracción de Strings.
            if(c=='"'){
                string texto="\"";
                i++;

                // Mientras no lleguemos al final de la línea (i<n) Y el caracter actual no sea la comilla de cierre (!='"').
                while(i<n&&linea[i]!='"'){
                    texto+=linea[i];
                    i++;
                }

                // Si el bucle while terminó porque 'i' llegó a ser igual a 'n', 
                // significa que leímos toda la línea y NUNCA encontramos la comilla de cierre.
                if(i==n){
                    cout<<"Error lexico: Comillas no cerradas en la linea "<<numLinea<<endl;
                    encontroErrores=true;
                    // Al no haber 'break' aquí, simplemente pasará a la siguiente línea del archivo.
                }else{
                    texto+='"';
                    i++;

                    Token nuevoToken;
                    nuevoToken.lexema=texto;
                    nuevoToken.tipo="LITERAL_STRING";
                    listaTokens.push_back(nuevoToken);
                }
                continue;
            }

            // EXPLICACIÓN: Extracción de Identificadores y Keywords.
            if(isalpha(c)||c=='_'||c=='#'){
                string palabra="";
                while(i<n&&(isalnum(linea[i])||linea[i]=='_'||linea[i]=='#')){
                    palabra+=linea[i];
                    i++;
                }

                Token nuevoToken;
                nuevoToken.lexema=palabra;

                if(esKeyword(palabra)){
                    nuevoToken.tipo="KEYWORD";
                    listaTokens.push_back(nuevoToken);
                }else{
                    nuevoToken.tipo="IDENTIFIER";
                    listaTokens.push_back(nuevoToken);

                    // Agrega el identificador a la tabla de símbolos solo si no existe ya
                    if(find(tablaSimbolos.begin(),tablaSimbolos.end(),palabra)==tablaSimbolos.end()){
                        tablaSimbolos.push_back(palabra);
                    }
                }
                continue;
            }

            // Extracción de números (Literales Enteros)
            if(isdigit(c)){
                string numero="";
                while(i<n&&isdigit(linea[i])){
                    numero+=linea[i];
                    i++;
                }
                Token nuevoToken;
                nuevoToken.lexema=numero;
                nuevoToken.tipo="LITERAL_INT";
                listaTokens.push_back(nuevoToken);
                continue;
            }

            // EXPLICACIÓN: Operadores de dos caracteres (<< y >>). 
            if(c=='<'&&i+1<n&&linea[i+1]=='<'){
                Token nuevoToken;
                nuevoToken.lexema="<<";
                nuevoToken.tipo="OPERATOR";
                listaTokens.push_back(nuevoToken);
                i+=2;
                continue;
            }

            if(c=='>'&&i+1<n&&linea[i+1]=='>'){
                Token nuevoToken;
                nuevoToken.lexema=">>";
                nuevoToken.tipo="OPERATOR";
                listaTokens.push_back(nuevoToken);
                i+=2;
                continue;
            }

            // Detección de operadores simples de un solo carácter
            if(c=='='||c=='+'||c=='-'||c=='*'||c=='/'||c=='<'||c=='>'){
                string op(1,c);
                Token nuevoToken;
                nuevoToken.lexema=op;
                nuevoToken.tipo="OPERATOR";
                listaTokens.push_back(nuevoToken);
                i++;
                continue;
            }

            // Detección de delimitadores de agrupación y fin de sentencia
            if(c==';'||c=='('||c==')'||c=='{'||c=='}'||c==','){
                string del(1,c);
                Token nuevoToken;
                nuevoToken.lexema=del;
                nuevoToken.tipo="DELIMITER";
                listaTokens.push_back(nuevoToken);
                i++;
                continue;
            }

            // SI LLEGA AQUÍ ES UN ERROR LÉXICO
            cout<<"Error lexico: Caracter no valido '"<<c<<"' en la linea "<<numLinea<<endl;
            encontroErrores=true;
            i++; // Avanzamos el cursor para que pueda seguir buscando más errores en la misma línea
        }
    }

    // Cierra el archivo de texto una vez analizado
    archivo.close();

    // SOLO SI NO HUBO NINGÚN ERROR EN TODO EL ARCHIVO, IMPRIME LAS TABLAS
    if(!encontroErrores){
        cout<<"--- Tabla lexema - token ---"<<endl;
        for(int j=0;j<listaTokens.size();j++){
            cout<<listaTokens[j].lexema<<" -> "<<listaTokens[j].tipo<<endl;
        }

        cout<<"\n--- Tabla de simbolos (Variables sin duplicar) ---"<<endl;
        for(int k=0;k<tablaSimbolos.size();k++){
            cout<<tablaSimbolos[k]<<endl;
        }
    }

    return 0;
}
