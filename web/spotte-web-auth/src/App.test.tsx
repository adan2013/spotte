import { render, screen } from '@testing-library/react'
import App from './App.tsx'

describe('App', () => {
  it('should render the app', () => {
    render(<App />)
    expect(screen.getByText('Hello world from TailwindCSS!')).toBeVisible()
  })
})
