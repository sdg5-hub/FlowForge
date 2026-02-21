import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { Reminder } from '@/types';

type DateCardProps = {
  date: string;
  reminders: Reminder[];
};

export default function DateCard({ date, reminders }: DateCardProps) {
  return (
    <View style={styles.card}>
      <Text style={styles.dateText}>{date}</Text>
      {reminders.map((r, i) => (
        <Text key={i} style={styles.reminderText}>
          {r.time} — {r.text}
        </Text>
      ))}
    </View>
  );
}

const styles = StyleSheet.create({
  card: {
    padding: 15,
    borderRadius: 8,
    backgroundColor: '#f0f0f0',
    marginVertical: 8,
    marginHorizontal: 10,
  },
  dateText: { fontSize: 18, fontWeight: 'bold', marginBottom: 10 },
  reminderText: { fontSize: 16, marginBottom: 4 },
});