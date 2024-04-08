# STL-like ассоциативный контейнер на бинарном дереве поиска

## Задача

Реализовать STL-совместимый контейнер для [BinarySearchTree](https://en.wikipedia.org/wiki/Binary_search_tree), реализующие различные [способы обхода дерева (in-, pre-, post-order)](https://en.wikipedia.org/wiki/Tree_traversal) через итератор.

## Требования

Контейнер предоставляет из себя шаблон, параметризуемый типом хранимых объектов и оператором сравнения, а так же удовлетворяет следующим требованиям к stl - совместимым контейнерам:

- [контейнера](https://en.cppreference.com/w/cpp/named_req/Container)
- [контейнера с обратным итератором](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer)
- [контейнера поддерживающие аллокатор](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)
- [oбладать двунаправленным итератом](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator)

Способ обхода дерева реализован через итератор, те оператор "++" приводит к перемещению итератора к следующему элементу в дереве, согласно правилу обхода (preorder, inorder, postorder).

## Тесты

К проекту написано несколько тестов в фреймворке *google test*
