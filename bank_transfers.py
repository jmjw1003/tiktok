from dataclasses import dataclass

@dataclass
class BankAccount:
    name: str
    balance: float = 0.0

    def deposit(self, amount: float) -> float:
        self.balance += amount
        return self.balance
    
    def withdraw(self, amount: float) -> float:
        if amount > self.balance:
            raise ValueError("Insufficient funds")
        self.balance -= amount
    
    def transfer(self, other: 'BankAccount', amount: float) -> 'Transfer':
        if amount <= self.balance:
            self.withdraw(amount)
            other.deposit(amount)
            return Transfer(self, other, amount)
        else:
            raise ValueError("Insufficient funds")
        

@dataclass
class Transfer:
    sender: BankAccount
    receiver: BankAccount
    amount: float

    def __str__(self):
        return f"{self.sender.name} transferred {self.amount} to {self.receiver.name}"
    
    def __repr__(self):
        return f"Transfer({self.sender.name}, {self.receiver.name}, {self.amount})"
    
    def reverse(self):
        self.sender.deposit(self.amount)
        self.receiver.withdraw(self.amount)
        return Transfer(self.receiver, self.sender, self.amount)


def main():
    acc1 = BankAccount("Alice")
    acc2 = BankAccount("Bob")
    acc1.deposit(100)
    acc2.deposit(50)
    print(acc1)
    print(acc2)
    t = acc1.transfer(acc2, 25)
    print(t)
    print(acc1)
    print(acc2)
    print("Reversing transfer")
    t.reverse()
    print(acc1)
    print(acc2)


if __name__ == "__main__":
    main()
    #TODO: Add second part of interview question: balancing accounts to have at least 100 in each.
