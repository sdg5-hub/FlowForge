import { View, Text, StyleSheet } from 'react-native';
import CalendarComponent from '@/components/calendar-component';

export default function HomeScreen() {
  return (
    <View style={styles.container}>
      <CalendarComponent />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingTop: 50,
  },
});
