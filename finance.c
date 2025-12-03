#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "finance.h"
#include "utils.h"
#include "colors.h"

#define MAX_TRADES 50

typedef struct {
    char pair[10];
    char dir[5];
    double entry;
    double exit;
    double lots;
    double pnl;
} Trade;

static Trade trades[MAX_TRADES];
static int trade_count = 0;

extern double get_double(char *p, int pos);
extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);

static void load_trades(void) {
    FILE *f = fopen("trade_journal.txt", "r");
    if (!f) return;
    trade_count = 0;
    while (trade_count < MAX_TRADES && 
           fscanf(f, "%9[^|]|%4[^|]|%lf|%lf|%lf|%lf\n",
                  trades[trade_count].pair, trades[trade_count].dir,
                  &trades[trade_count].entry, &trades[trade_count].exit,
                  &trades[trade_count].lots, &trades[trade_count].pnl) == 6) {
        trade_count++;
    }
    fclose(f);
}

static void save_trades(void) {
    FILE *f = fopen("trade_journal.txt", "w");
    if (!f) return;
    for (int i = 0; i < trade_count; i++) {
        fprintf(f, "%s|%s|%.5f|%.5f|%.2f|%.2f\n",
                trades[i].pair, trades[i].dir, trades[i].entry,
                trades[i].exit, trades[i].lots, trades[i].pnl);
    }
    fclose(f);
}

static void print_risk_bar(double risk_pct) {
    int bars = (int)risk_pct;
    if (bars > 10) bars = 10;
    if (bars < 0) bars = 0;
    
    char *color = risk_pct < 1.5 ? COLOR_GREEN : risk_pct < 3.0 ? COLOR_YELLOW : COLOR_RED;
    char *label = risk_pct < 1.5 ? "LOW" : risk_pct < 3.0 ? "MEDIUM" : "HIGH";
    
    printf("Risk: %.1f%% %s[", risk_pct, color);
    for (int i = 0; i < 10; i++) printf(i < bars ? "#" : "-");
    printf("]" COLOR_RESET " %s%s" COLOR_RESET "\n", color, label);
}

void trade_calculator(void) {
    print_header("Trade Calculator");
    
    double account = get_double("Account balance ($): ", 0);
    double risk_pct = get_double("Risk per trade (%%): ", 0);
    double sl_pips = get_double("Stop loss (pips): ", 0);
    
    printf("\nPair type:\n1. USD pairs (EURUSD, GBPUSD)\n2. JPY pairs (USDJPY)\n3. Other\nChoice: ");
    int pair_type = get_int("", 1, 3);
    
    double pip_val = pair_type == 1 ? 10.0 : pair_type == 2 ? 6.67 : get_double("Pip value/lot: ", 0);
    
    double risk_amt = account * risk_pct / 100.0;
    double lots = risk_amt / (sl_pips * pip_val);
    double margin = lots * 100000.0 / 100.0;
    
    printf("\n" COLOR_CYAN "=== Results ===" COLOR_RESET "\n\n");
    print_risk_bar(risk_pct);
    printf("\nRisk Amount:   $%.2f\n", risk_amt);
    printf("Position:      %.2f lots (%.0f micro)\n", lots, lots * 100);
    printf("Pip Value:     $%.2f/pip\n", pip_val * lots);
    printf("Margin:        $%.2f\n\n", margin);
    
    if (lots > 0.5) printf("Warning: Large position\n");
    else if (lots < 0.01) printf("Warning: Very small position\n");
    
    go_back();
}

void risk_analyzer(void) {
    print_header("Risk Analyzer");
    
    char pair[10];
    printf("Pair (e.g. EURUSD): ");
    scanf(" %9s", pair);
    flush_input();
    
    double entry = get_double("Entry: ", 0);
    double sl = get_double("Stop loss: ", 0);
    double tp = get_double("Take profit: ", 0);
    double lots = get_double("Lots: ", 0);
    double account = get_double("Account ($): ", 0);
    
    printf("\n1. Long (BUY)\n2. Short (SELL)\nChoice: ");
    int dir = get_int("", 1, 2);
    
    double sl_dist = dir == 1 ? entry - sl : sl - entry;
    double tp_dist = dir == 1 ? tp - entry : entry - tp;
    double rr = tp_dist / sl_dist;
    
    double loss = sl_dist * 10000.0 * 10.0 * lots;
    double profit = tp_dist * 10000.0 * 10.0 * lots;
    double risk_pct = (loss / account) * 100.0;
    
    printf("\n" COLOR_CYAN "=== Analysis ===" COLOR_RESET "\n\n");
    print_risk_bar(risk_pct);
    
    printf("\nStop Loss:     %.1f pips ($%.2f)\n", sl_dist * 10000, loss);
    printf("Take Profit:   %.1f pips ($%.2f)\n", tp_dist * 10000, profit);
    printf("Risk:Reward:   1:%.2f\n\n", rr);
    
    char *rating = rr >= 3.0 ? COLOR_GREEN "Excellent" : 
                   rr >= 2.0 ? COLOR_GREEN "Good" :
                   rr >= 1.5 ? COLOR_YELLOW "Acceptable" : COLOR_RED "Poor";
    printf("%s" COLOR_RESET "\n", rating);
    printf("Breakeven win rate: %.1f%%\n", 100.0 / (1.0 + rr));
    
    if (get_yn("\nLog to journal? (y/n): ") == 'y') {
        double exit_price = get_double("Exit price (0 if open): ", 0);
        if (exit_price > 0 && trade_count < MAX_TRADES) {
            load_trades();
            strcpy(trades[trade_count].pair, pair);
            strcpy(trades[trade_count].dir, dir == 1 ? "BUY" : "SELL");
            trades[trade_count].entry = entry;
            trades[trade_count].exit = exit_price;
            trades[trade_count].lots = lots;
            
            double diff = exit_price - entry;
            if (dir == 2) diff = -diff;
            trades[trade_count].pnl = diff * 10000.0 * 10.0 * lots;
            
            trade_count++;
            save_trades();
            printf("Trade logged\n");
        }
    }
    go_back();
}

void trade_journal(void) {
    print_header("Trade Journal");
    load_trades();
    
    double total = 0;
    int wins = 0;
    for (int i = 0; i < trade_count; i++) {
        total += trades[i].pnl;
        if (trades[i].pnl > 0) wins++;
    }
    
    if (trade_count > 0) {
        printf("Trades: %d | Wins: %d | Rate: %.1f%%\n", 
               trade_count, wins, (double)wins / trade_count * 100);
        printf("Total: %s$%.2f" COLOR_RESET "\n\n", 
               total >= 0 ? COLOR_GREEN : COLOR_RED, total);
    }
    
    printf("1. Log trade\n2. View history\n3. Export CSV\n4. Clear\n0. Back\nChoice: ");
    int c = get_int("", 0, 4);
    
    if (c == 0) return;
    
    if (c == 1 && trade_count < MAX_TRADES) {
        Trade *t = &trades[trade_count];
        printf("Pair: ");
        scanf(" %9s", t->pair);
        printf("Direction (BUY/SELL): ");
        scanf(" %4s", t->dir);
        flush_input();
        
        t->entry = get_double("Entry: ", 0);
        t->exit = get_double("Exit: ", 0);
        t->lots = get_double("Lots: ", 0);
        
        double diff = t->exit - t->entry;
        if (t->dir[0] == 'S' || t->dir[0] == 's') diff = -diff;
        t->pnl = diff * 10000.0 * 10.0 * t->lots;
        
        trade_count++;
        save_trades();
        printf("\nP/L: %s$%.2f" COLOR_RESET "\n", t->pnl >= 0 ? COLOR_GREEN : COLOR_RED, t->pnl);
        printf("Trade logged\n");
        
    } else if (c == 2 && trade_count > 0) {
        printf("\n%-10s %-5s %-10s %-10s %s\n", "Pair", "Dir", "Entry", "Exit", "P/L");
        printf("------------------------------------------------\n");
        int start = trade_count > 10 ? trade_count - 10 : 0;
        for (int i = start; i < trade_count; i++) {
            printf("%-10s %-5s %-10.5f %-10.5f %s%.2f" COLOR_RESET "\n",
                   trades[i].pair, trades[i].dir, trades[i].entry, 
                   trades[i].exit, trades[i].pnl >= 0 ? COLOR_GREEN : COLOR_RED, 
                   trades[i].pnl);
        }
        
    } else if (c == 3 && trade_count > 0) {
        FILE *f = fopen("trades.csv", "w");
        if (f) {
            fprintf(f, "Pair,Direction,Entry,Exit,Lots,PnL\n");
            for (int i = 0; i < trade_count; i++) {
                fprintf(f, "%s,%s,%.5f,%.5f,%.2f,%.2f\n",
                        trades[i].pair, trades[i].dir, trades[i].entry,
                        trades[i].exit, trades[i].lots, trades[i].pnl);
            }
            fclose(f);
            printf("Exported to trades.csv\n");
            if (get_yn("Generate graphs? (y/n): ") == 'y') {
                system("python3 plot_forex.py trades.csv");
            }
        }
        
    } else if (c == 4 && get_yn("Clear all? (y/n): ") == 'y') {
        trade_count = 0;
        save_trades();
        printf("Journal cleared\n");
    }
    go_back();
}

void quick_reference(void) {
    print_header("Quick Reference");
    
    printf(COLOR_CYAN "Lot Sizes:" COLOR_RESET "\n");
    printf("Standard: 1.00 = 100,000 units\n");
    printf("Mini:     0.10 = 10,000 units\n");
    printf("Micro:    0.01 = 1,000 units\n\n");
    
    printf(COLOR_CYAN "Pip Values (per lot):" COLOR_RESET "\n");
    printf("EURUSD/GBPUSD:  $10.00\n");
    printf("USDJPY:         $6.67\n");
    printf("XAUUSD:         $10.00\n\n");
    
    printf(COLOR_CYAN "Risk Rules:" COLOR_RESET "\n");
    printf("Max per trade:  1-2%% account\n");
    printf("Min R:R:        1:2\n");
    printf("Daily limit:    5%% account\n");
    
    go_back();
}

void finance_suite(void) {
    int c;
    while (1) {
        print_header("Forex Trading Suite");
        printf("1. Trade Calculator\n2. Risk Analyzer\n3. Trade Journal\n");
        printf("4. Quick Reference\n0. Back\nChoice: ");
        
        if (scanf("%d", &c) != 1 || c < 0 || c > 4) {
            flush_input();
            printf("\nInvalid input, try 0-4\n");
            printf("Press Enter to continue");
            getchar();
            continue;
        }
        flush_input();
        
        if (c == 1) trade_calculator();
        else if (c == 2) risk_analyzer();
        else if (c == 3) trade_journal();
        else if (c == 4) quick_reference();
        else return;
    }
}