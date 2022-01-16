package main.game;

public class Card {
    enum type {
        diamoind(3);
        int id;

        type(int i) {
            id = i;
        }
    }
}
