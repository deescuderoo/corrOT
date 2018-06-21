# Correlated OT

This is an implementation of the Correlated Oblivious Transfer protocol presented in Section 4 of [S18]. It uses [libscapi](https://github.com/cryptobiu/libscapi) for the communication layer, as well as for the base OT protocol (these are wrappers for [Boost](https://www.boost.org/)'s communication and [Simplest OT](https://eprint.iacr.org/2015/267.pdf), respectively)

[S18] Peter Scholl: Extending Oblivious Transfer with Low Communication via Key-Homomorphic PRFs. Public Key Cryptography (1) 2018: 554-583
