#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <random>
#include <algorithm>

using namespace std;

// Функция для генерации супер-увеличивающейся последовательности (Наш закрытый ключ)
vector<int> generateSuperIncreasingSeq(int size) {
    vector<int> superIncreasingSeq;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);  // Случайные числа от 1 до 10

    int sum = 0;
    for (int i = 0; i < size; ++i) {
        int num = dis(gen) + sum;
        superIncreasingSeq.push_back(num);
        sum += num;
    }
    return superIncreasingSeq;
}

// Создаем Открытый и Закрытый ключи
void generateKeys(vector<int>& privateKey, vector<int>& publicKey, int& modulo, int& multiplier) {
    cout << "Введите размер супер-увеличивающейся последовательности (рекомендуется 9): ";
    int size;
    cin >> size;

    // Генерация закрытого ключа в виде супер-увеличивающейся последовательности с использованием внешней функции
    privateKey = generateSuperIncreasingSeq(size);

    cout << "Закрытый ключ (супер-увеличивающаяся последовательность): ";
    for (int num : privateKey) {
        cout << num << " ";
    }
    cout << endl;
    
    //Запрашиваем модуль ключа и множитель
    do {
        cout << "Введите значение модуля (должно быть больше любого элемента закрытого ключа): ";
        cin >> modulo;
    } while (modulo <= *max_element(privateKey.begin(), privateKey.end()));

    cout << "Введите множитель (должен быть взаимно простым с модулем): ";
    cin >> multiplier;

    // Генерация открытого ключа из закрытого ключа с использованием модульной арифметики
    for (int i = 0; i < privateKey.size(); ++i) {
        int publicKeyElement = (privateKey[i] * multiplier) % modulo;
        publicKey.push_back(publicKeyElement);
    }
    
    //Выводим ключ
    cout << "Открытый ключ (последовательность): ";
    for (int num : publicKey) {
        cout << num << " ";
    }
    cout << endl;
}

// Зашифровать строку, используя открытый ключ
pair<vector<int>, vector<string>> encrypt(const vector<int>& superIncreasingSeq, const string& text) {
    vector<int> encryptedText;
    vector<string> binaryValues;

    for (char c : text) {
        // Преобразование текущего символа в его 8-битное представление
        string binaryVal = bitset<8>(c).to_string();  // Бинарное представление текущего символа
        binaryValues.push_back(binaryVal);  // Сохранение бинарного представления

        int sum = 0;
        // Проверка возможности зашифровать текст с использованием открытого ключа, иначе - ошибка
        for (int i = 0; i < 8; ++i) {
            // Побитовое добавление суммы, если бит равен единице
            if ((c >> (7 - i)) & 1) {  // Если бит равен 1
                if (i < superIncreasingSeq.size()) {
                    sum += superIncreasingSeq[i];
                }
                else {
                    cerr << "Размер супер-увеличивающейся последовательности слишком мал для входного текста." << endl;
                    return make_pair(vector<int>(), vector<string>());
                }
            }
        }
        encryptedText.push_back(sum);  // Добавление к зашифрованному тексту
    }
    // Возврат зашифрованного текста
    return make_pair(encryptedText, binaryValues);
}

// Расшифровать зашифрованный вектор, используя закрытый ключ
pair<string, string> decrypt(const vector<int>& superIncreasingSeq, int multiplier, int modulo, const vector<int>& encryptedText) {
    // Ищем обратный множитель
    int inverseMultiplier = 0;
    while ((multiplier * inverseMultiplier) % modulo != 1) {  // Нахождение модулярного мультипликативного обратного
        inverseMultiplier++;
    }
    //Умножаем зашифрованное значение на обратный множитель и расшифровываем его используя закрытый ключи
    string decryptedText = "";
    string decryptedBinary = "";
    for (int num : encryptedText) {
        int decryptedNum = (num * inverseMultiplier) % modulo;  // Расшифровка числа

        string binaryString = "";  // Переменная для хранения бинарной строки для символа
        for (int i = superIncreasingSeq.size() - 1; i >= 0; --i) {
            if (decryptedNum >= superIncreasingSeq[i]) {
                decryptedNum -= superIncreasingSeq[i];
                binaryString = "1" + binaryString;  // Добавление "1" к бинарной строке
            }
            else {
                binaryString = "0" + binaryString;  // Добавление "0" к бинарной строке
            }
        }

        // Преобразование бинарной строки в символ и добавление к расшифрованному тексту
        decryptedText += static_cast<char>(bitset<8>(binaryString).to_ulong());
        decryptedBinary += binaryString + " ";  // Разделение бинарных символов пробелом
    }

    return make_pair(decryptedText, decryptedBinary);
}

// Шифруем и дешифруем используя систему на основе задачи о рюкзаке
void encrypt_n_decrypt(vector<int>& privateKey, vector<int>& publicKey, int& modulo, int& multiplier){
    // Ввод пользоватем текста, который мы будем шифровать
    string text;
    cout << "Введите текст для шифрования: ";
    cin.ignore();  // Игнорирование символа новой строки в буфере
    getline(cin, text);

    // Зашифровка введенного пользователем текста с использованием сгенерированных ключей
    auto encrypted = encrypt(publicKey, text);

    cout << "\nЗашифрованное сообщение:" << endl;
    for (int num : encrypted.first) {
        cout << num << " ";
    }
    cout << endl;

    // Расшифровка зашифрованного сообщения
    auto decrypted = decrypt(privateKey, multiplier, modulo, encrypted.first);
    cout << "\nРасшифрованное сообщение: " << decrypted.first << endl;
}


int main() {
    bool cont = true;
    vector<int> privateKey;
    vector<int> publicKey;
    int modulo; 
    int multiplier;
    
    //Создаем ключи
    generateKeys(privateKey, publicKey, modulo, multiplier);
    //Шифруем и расшифровываем, результат выводим в строку
    encrypt_n_decrypt(privateKey, publicKey, modulo, multiplier);
    
    
    //Интерфейс работы с данной программой.
    int user_input = 0;
    while(cont == true){
        cout << "What to do next:\n1. generate new set of keys \n2. encrypt and decrypt another messagee\n3. exit\n";
        cin >> user_input;
        switch(user_input){
            //Новый ключ
            case 1 :{
                privateKey.clear();
                publicKey.clear();
                generateKeys(privateKey, publicKey, modulo, multiplier);
                break;
            }
            //Работа с другим текстом
            case 2 :{
                encrypt_n_decrypt(privateKey, publicKey, modulo, multiplier);
                break;
            }
            case 3 :{
                cont = false;
                break;
            }
            default :{
                cout << "Invalid input\n";
                break;
            }
        }
        
    }
    return 0;
}
